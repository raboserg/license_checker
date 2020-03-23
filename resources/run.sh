export BOOST_ROOT="$HOME/projects/library/ITVPN_DEV/build"
export ACE_ROOT="$HOME/projects/library/ITVPN_DEV/ACE_wrappers"
export LD_LIBRARY_PATH="$PWD:$ACE_ROOT/lib:BOOST_ROOT/lib:$LD_LIBRARY_PATH"

sudo ./itvpnagent

