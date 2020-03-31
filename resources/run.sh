#export BOOST_ROOT="$HOME/projects/library/ITVPN_DEV/build"
#export ACE_ROOT="$HOME/projects/library/ITVPN_DEV/ACE_wrappers"
#export LD_LIBRARY_PATH="$PWD:$ACE_ROOT/lib:BOOST_ROOT/lib:$LD_LIBRARY_PATH"

while getopts i:r: opts; do
   case ${opts} in
      i) 
	#FROM_VAL=${OPTARG} 
	#echo ${FROM_VAL}
	sudo ln -s  ~/itvpnagent/itvpnagent.service /etc/systemd/system/itvpnagent.service
	sudo systemctl start itvpnagent
	sudo systemctl -l status itvpnagent
	;;
      r) 
	#TO_VAL=${OPTARG} 
	#echo ${TO_VAL}
	sudo systemctl stop itvpnagent
	sudo rm  /etc/systemd/system/itvpnagent.service
	sudo systemctl -l status itvpnagent
	;;
   esac
done


case "$1" in
	i)
	sudo ln -s  ~/itvpnagent/itvpnagent.service /etc/systemd/system/itvpnagent.service
	sudo systemctl start itvpnagent
	sudo systemctl -l status itvpnagent
    ;;
  r)
	sudo systemctl stop itvpnagent
	sudo rm  /etc/systemd/system/itvpnagent.service
	sudo systemctl -l status itvpnagent
    ;;
  *)
    echo "Usage: run.sh {i|r}"
    exit 1
esac
