# TLSv1-scan.sh script
# Written by Gratien D'haese
# Version 1.0 (date 23/Mar/2026)

PRGNAME="TLSv1-scan"
VERSION="1.0"
DATE="23/Mar/2026"
PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/local/sbin
export PATH

handle_interrupt() {
	echo -e "\nCtrl+C was pressed. Exit now."
	exit 1
}

echo "
******************************************
* $PRGNAME by Gratien D'haese
* Version $VERSION
* Date $DATE
******************************************
"

if [[ $(id -u) -ne 0 ]] ; then
	echo "Program $PRGNAME needs to be run as root."
	exit 1
fi

trap handle_interrupt SIGINT

type -p sslscan >/dev/null 2>&1
if [[ $? -eq 1 ]] ; then
	echo "Please install 'sslscan' first (part of EPEL)!"
	exit 1
fi

type -p ss >/dev/null 2>&1
if [[ $? -eq 1 ]] ; then
	echo "Please install 'ss' first (part of iproute package)!"
	exit 1
fi

type -p lsof >/dev/null 2>&1
if [[ $? -eq 1 ]] ; then
        echo "Please install 'lsof' first (part of lsof package)!"
        exit 1
fi


for host_port in $( netstat -an | grep LISTEN | grep -v LISTENING | grep -v tcp6  | awk '{print $4}' )
do
	echo "**** Scanning for TLS in IP:port $host_port ****"
	HOST=${host_port%:*}
        PORT=${host_port##*:}
	timeout 10 sslscan ${HOST}:${PORT} | grep -E '(TLSv1.0|TLSv1.1)' | grep enable
        if [[ $? -eq 0 ]] ; then
		echo "What is behind port $PORT?"
		ss -tnlp | grep $PORT
		echo "Which program is using PID $PID?"
		PID=$(ss -tnlp | grep $PORT | awk '{print $6}' | cut -d= -f2 | cut -d, -f1)
		lsof -p $PID | head -2
		echo "------------------------------------------------------------------------------------"
        fi
done

