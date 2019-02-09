#!/bin/sh

# environs=`pidof dbus-daemon | tr ' ' '\n' | awk '{printf "/proc/%s/environ ", $1}'`
# export DBUS_SESSION_BUS_ADDRESS=`cat $environs 2>/dev/null | tr '\0' '\n' | grep DBUS_SESSION_BUS_ADDRESS | cut -d '=' -f2-`
# export DISPLAY=:0

# notify-send "Cron" "The storePings.sh has started";
echo "Strarted at [$(date)] : Cron triggered" >> ~/Desktop/cronLog; 
resultDir=~/Downloads/coding/networksLab/resultFiles;
ipList="supercell.com techfest.org techniche.org  github.com codeforces.com ubuntu.com steampowered.com cricbuzz.com billboard.com uefa.com";
ips="techniche.org ubuntu.com"
p="uday@ubuntu";

# start openVPN connection to ping outside IPs
echo "[$(date)] $(echo $p | sudo -S nmcli con up uday)" >> ~/Desktop/cronLog; 

# part 1 of Q2
for ip in $ipList;
do
	# For each append we have to check for packet loss, 
	# also need to collet avg time and correlate with distance
	echo "" >> ${resultDir}/ping_${ip}.txt;
	echo "Appending ping results from $(date)" >> ${resultDir}/ping_${ip}.txt;
	echo "[$(date)] Appending outputs to ping_${ip}.txt" >> ~/Desktop/cronLog;
	ping -c 20 $ip >> ${resultDir}/ping_${ip}.txt;
done

# notify-send "Cron" "ping part 1 done"


# part 2 of Q2
for ip in $ips; do

	for size in 64 128 256 512 1024 2048
	do
		# plot avg RTT
		echo "" >> ${resultDir}/ping_${ip}_${size}.txt;
		echo "Appending ping results from $(date)" >> ${resultDir}/ping_${ip}_${size}.txt;
		echo "[$(date)] Appending outputs to ping_${ip}_${size}.txt" >> ~/Desktop/cronLog;
		ping -c 20 -s $size $ip >> ${resultDir}/ping_${ip}_${size}.txt;
	done

done;

# notify-send "Cron" "ping part 2 done"

# Q6
echo "Appending traceroute counts from $(date)" >> ${resultDir}/trCounts.txt;
for ip in $ipList;
do
	traceroute $ip > ${resultDir}/tr_temp.txt;
	echo "[$(date)] Appending traceroutes to tr_${ip}.txt" >> ~/Desktop/cronLog;
	echo "" >> ${resultDir}/tr_${ip}.txt;
	echo "Appending traceroute results from $(date)" >> ${resultDir}/tr_${ip}.txt;
	cat ${resultDir}/tr_temp.txt >> ${resultDir}/tr_${ip}.txt;
	trCount=$(($(cat ${resultDir}/tr_temp.txt | wc -l)-1));
	echo $(date),$ip,  $trCount >> ${resultDir}/trCounts.txt;
	echo "[$(date)] Appended tr counts trCounts.txt : $ip,  $trCount" >> ~/Desktop/cronLog;
done

# notify-send "Cron" "traceroutes done, last trCount: $trCount"

# done accessing outside stuff, down it again.
echo "[$(date)] $(echo $p | sudo -S nmcli con down uday)" >> ~/Desktop/cronLog; 
# echo $p | sudo -S service squid restart;

# Q8
nMapRange="10.1.2.0/24";
nmap -n -sP ${nMapRange} >  ${resultDir}/nmap_temp.txt;
echo "Appending nmap results from $(date)" >> ${resultDir}/nmapdump.txt;
cat  ${resultDir}/nmap_temp.txt >> ${resultDir}/nmapdump.txt;
upCount=$( cat  ${resultDir}/nmap_temp.txt | grep -c "Host is up");
echo $(date), $upCount >> ${resultDir}/nmapCounts.txt
echo "Finished at [$(date)] Appending nmap counts : $upCount to nmapCounts.txt" >> ~/Desktop/cronLog;
# notify-send "Cron finished" "nmap scanning done, $upCount hosts are up"
