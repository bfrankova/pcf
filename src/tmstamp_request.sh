IP="192.168.0.100"
if [ $# -eq 1 ];
then
  IP=$1
fi

while [ 1 ]; do
	sudo icmpush -tstamp $IP
done
