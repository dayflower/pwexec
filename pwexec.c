#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <glib.h>
#include <gnome-keyring.h>

static const char  APPVER_KEY[] = "pwexec_ver";
static const guint APPVER_VALUE = 1;
static const char  REALM[]      = "realm";

static GnomeKeyringPasswordSchema pwexec_schema = {
    GNOME_KEYRING_ITEM_GENERIC_SECRET,
    {
        { APPVER_KEY, GNOME_KEYRING_ATTRIBUTE_TYPE_UINT32 },
        { REALM,      GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
        { NULL, 0 },
    }
};

static const gchar *action  = NULL;
static const gchar *keyring = GNOME_KEYRING_SESSION;
static const gchar *realm   = NULL;

static void
usage(void)
{
    fputs("usage: pwexec [OPTIONS] <action> [<command> <args> ...]\n", stderr);
}

int
main(int argc, char *argv[])
{
    static struct option options[] = {
        /* options */
        { "keyring", required_argument, NULL, 'k' },
        { "realm",   required_argument, NULL, 'r' },

        /* command, can be set as option form */
        { "set",     no_argument,       NULL, 'S' },
        { "delete",  no_argument,       NULL, 'D' },
        { "exec",    no_argument,       NULL, 'X' },

        /* help */
        { "help",    no_argument,       NULL, '?' },

        { NULL, 0, NULL, 0 }
    };
    GnomeKeyringResult keyres;

    g_set_application_name("pwexec");

    for (;;) {
        int c, final = 0;

        c = getopt_long(argc, argv, "-k:r:", options, NULL);
        if (c < 0)
            break;

        switch (c) {

        case 0:
        case 1:
            if (optind <= 1)
                return 1;
            action = argv[optind - 1];
            final = 1;
            break;

        case 'S':
        case 'D':
        case 'X':
            if (optind <= 1)
                return 1;
            action = argv[optind - 1] + 2;  /* skip prefix '--' */
            final = 1;
            break;

        case 'k':
            keyring = optarg;
            break;
        case 'r':
            realm = optarg;
            break;

        case '?':
            usage();
            return 1;
        }

        if (final)
            break;
    }

    if (! action
     || (strcmp(action, "exec")
      && strcmp(action, "set")
      && strcmp(action, "delete"))) {
        fputs("error: ", stderr);
        if (action)
            fputs("unsupported action was specified; ", stderr);
        else
            fputs("action was not specified; ", stderr);
        fputs("action can be one of 'exec', 'set', 'delete'.\n", stderr);
        usage();
        return 1;
    }

    if (! realm) {
        fputs("error: realm was not specified.\n", stderr);
        return 1;
    }

    if (! strcmp(action, "delete")) {
        keyres = gnome_keyring_delete_password_sync(&pwexec_schema,
                                                    APPVER_KEY, APPVER_VALUE,
                                                    REALM,      realm,
                                                    NULL);
        if (keyres != GNOME_KEYRING_RESULT_OK) {
            fprintf(stderr, "failed to delete password: %s\n",
                            gnome_keyring_result_to_message(keyres));

            return keyres;
        }

        fputs("Successfully deleted password.\n", stderr);
        return 0;
    }
    else if (! strcmp(action, "set")) {
        gchar *password = NULL, *retype = NULL;
        gchar *name = NULL;

        /* you must specify keyring */
        if (! keyring) {
            fputs("error: kerring was not specified.\n", stderr);
            return 1;
        }
        if (keyring && ! strcasecmp(keyring, "default"))
            keyring = GNOME_KEYRING_DEFAULT;

        /* overwrite check */
        keyres = gnome_keyring_find_password_sync(&pwexec_schema,
                                                  &password,
                                                  APPVER_KEY, APPVER_VALUE,
                                                  REALM,      realm,
                                                  NULL);
        if (keyres == GNOME_KEYRING_RESULT_OK) {
            char buf[16];

            gnome_keyring_free_password(password);
            fputs("password already exists in specified realm.\n"
                  "Overwrite? [Y/n]", stderr);
            fflush(stderr);

            if (! fgets(buf, 16, stdin))
                return 0;
            g_strstrip(buf);

            /* not 'YES' => don't advance */
            if (*buf != 0 && strcasecmp(buf, "yes") && strcasecmp(buf, "y"))
                return 0;
        }

        password = g_strdup(getpass("Password: "));
        retype   =          getpass("Retype Password: ");
        if (! password || ! retype || strcmp(password, retype)) {
            fputs("error: password error\n", stderr);
            gnome_keyring_free_password(password);
            return 1;
        }

        name = g_strdup_printf("pwexec key for %s", realm);

        keyres = gnome_keyring_store_password_sync(&pwexec_schema,
                                                   keyring,
                                                   name,
                                                   password,
                                                   APPVER_KEY, APPVER_VALUE,
                                                   REALM,      realm,
                                                   NULL);
        gnome_keyring_free_password(password);
        g_free(name);
        if (keyres != GNOME_KEYRING_RESULT_OK) {
            fprintf(stderr, "failed to store password: %s\n",
                            gnome_keyring_result_to_message(keyres));

            return keyres;
        }

        fputs("Successfully stored password.\n", stderr);
        return 0;
    }
    else if (! strcmp(action, "exec")) {
        gchar *password = NULL;
        int i, pindex = -1, r;

        if (! argv[optind]) {
            fputs("error: command line was not specified.\n", stderr);
            usage();
            return 1;
        }

        for (i = optind; argv[i]; i ++) {
            if (! strcmp(argv[i], "%PASSWORD%"))
                pindex = i;
        }
        if (pindex < 0) {
            fputs("error: '%PASSWORD%' template was not specified"
                  " in command line.\n", stderr);
            usage();
            return 1;
        }

        keyres = gnome_keyring_find_password_sync(&pwexec_schema,
                                                  &password,
                                                  APPVER_KEY, APPVER_VALUE,
                                                  REALM,      realm,
                                                  NULL);
        if (keyres != GNOME_KEYRING_RESULT_OK) {
            fprintf(stderr, "failed to find password: %s\n",
                            gnome_keyring_result_to_message(keyres));

            return keyres;
        }

        /* overwrite password template */
        argv[pindex] = password;

        /* now, execute! */
        r = execvp(argv[optind], argv + optind);
        /* in successful condition, it doesn't reach */

        perror("failed to execute");

        gnome_keyring_free_password(password);

        return r;
    }

    return 0;
}

/* vi: set expandtab ts=4 sw=4 sts=0 : */
