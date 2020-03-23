# ~/.profile: executed by the command interpreter for login shells.
# This file is not read by bash(1), if ~/.bash_profile or ~/.bash_login
# exists.
# see /usr/share/doc/bash/examples/startup-files for examples.
# the files are located in the bash-doc package.

# the default umask is set in /etc/profile; for setting the umask
# for ssh logins, install and configure the libpam-umask package.
#umask 022

# if running bash
if [ -n "$BASH_VERSION" ]; then
    # include .bashrc if it exists
    if [ -f "$HOME/.bashrc" ]; then
	. "$HOME/.bashrc"
    fi
fi

# set PATH so it includes user's private bin directories
PATH="$HOME/bin:$HOME/.local/bin:/usr/local/go/bin:$PATH"

export GOPATH=$HOME/go
#export PATH="$PATH:/usr/local/go/bin"
export ACE_ROOT=$HOME/install/ACE_wrappers
export CPLUS_INCLUDE_PATH="$ACE_ROOT:$CPLUS_INCLUDE_PATH"
export LD_LIBRARY_PATH="$ACE_ROOT/lib:$LD_LIBRARY_PATH"

export CC=clang
export CXX=clang++
export ACE_DEBUG=true
export XDEBUG_CONFIG="idekey=PHPVC"
export PATH="$HOME/.cargo/bin:$GOPATH/bin:$HOME/.config/composer/vendor/bin:$ACE_ROOT/lib:~chainlink/tools/bin:$PATH:~/projects/python/fuzzingbook/"
export EDITOR=vi # setting default editor for mc

