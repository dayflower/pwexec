pwexec
======

Replace command line argument with a password stored in GNOME Keyring
and execute.

Usage
-----

    pwexec [OPTIONS] <ACTION> [<command> <args> ...]

ACTION is one of:

  * `set`: set password associated with specified realm
  * `exec`: execute specified command with password replacement
  * `delete`: delete password

OPTIONS are:

  * `--keyring` (`-k`):
    Specify keyring name.
    Default keyring is `session`,
    that means stored passwords will be vanished on GNOME session logout.
    So if you want to make passwords persistent, you should specify
    `--keyring` option with keyring name explicitly.
    `default` is default keyring used in GNOME system and recommended.
  * `--realm` (`-r`):
    Realm.

Example
-------

For example, imagine you will execute rdesktop program with password.

    $ rdesktop -u foo -p bar baz

At first, you have to store a password onto GNOME keyring.

    $ pwexec -r rdeskpw -k default set
    
    Password: ********
    Retype Password: ********
    Successfully stored password.

You can specify realm name (on above example, `rdeskpw`) whatever you want.

Then, execute the program.

    $ pwexec -r rdeskpw exec rdesktop -u foo -p %PASSWORD% baz

On exec phase, pwexec will replace `%PASSWORD%` argument with the password
stored on set phase and then execute the command.

You do not have to specify `--keyring` parameter on exec phase.

Caveat
------

This program does only replace command line argument simply,
so proc filesystem would reveal your replaced password.
Some programs (such as rdesktop) mask passwords on proc store,
but not all programs behave like that.

See also
--------

I wrote [the article](http://d.hatena.ne.jp/dayflower/20081205/1228464510)
(written in Japanese) about motivation.

Contributing
------------

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

Copyright and license
---------------------

Copyright © 2012 ITO Nobuaki.
See LICENSE for details (MIT License).
