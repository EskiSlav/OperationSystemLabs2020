#!/bin/bash
usage="script -d directory -t template -f filename"

Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

checkargs() {
	if [[ $OPTARG =~ ^-[d,t,f]$ ]]
	then
		echo "Unknown argument $OPTARG for option $option"
		exit 1
	else
		echo "lol"
	fi
}

directory=
template=
filename=
while getopts "d:t:f:" option 
	do
		case "$option" in 
			d) 	checkargs
			 	directory=${OPTARG};; 
			t) 	checkargs 
				template=${OPTARG};;
			f) 	checkargs 
				filename=${OPTARG};;
		esac
	done


if [ -d "$directory" ];
then 
echo "$directory valid"
else
echo "Such directory does not exists."
exit 1
fi 

if [[ $directory == "" || $template == "" ]];
then
	echo " [!] Usage: $usage"
	exit 1
fi

if [[ $filename == "" ]];
then
	read -p "Please enter the name of the file: "
	filename=$REPLY
fi

if [[ -f $filename ]];
then
	printf "This file: $filename already exists.\n What do you want to do?\n  [1] Rewrite file\n  [2] Append to file\n  [3] exit\n  Choice:"
	read  
	if [[ $REPLY == "1" ]];	then
		rm $filename
		touch $filename
	fi

	if [[ $REPLY == "3" ]]; then
		exit 1
	fi

	touch /tmp/$filename.err
	touch /tmp/$filename.bin
fi

printf '\n'
# echo -e "${Green}Output:${White}"
# printf '\n'
grep -nrwi $directory -e "$template" 2> /tmp/$filename.err > "/tmp/$filename.tmp"

while IFS="" read -r p || [ -n "$p" ]
do 
	if [[ "$p" =~ .*"matches"*. ]];
	then
		echo "$p" > "/tmp/$filename.bin"
	fi
done < /tmp/$filename.tmp 

sed -i -e '/matches/d' /tmp/$filename.tmp

while IFS="" read -r p || [ -n "$p" ]
do
name=$(cut -d: -f1 <<< "$p")
text=$(cut -d: -f3 <<< "$p")
echo "$(basename "$name"):$(realpath $name):$(file $name | cut -d: -f2):bytes $(wc -c $name | cut -d" " -f1):text $text" >> $filename

done < "/tmp/$filename.tmp"

while IFS="" read -r p || [ -n "$p" ]
do	
	name=$(cut -d" " -f3 <<< "$p")
	echo "$(basename "$name"):$(realpath $name):$(file $name | cut -d: -f2):bytes $(wc -c $name | cut -d" " -f1):binary file" >> $filename
done < /tmp/$filename.bin


rm "/tmp/$filename.tmp"
rm "/tmp/$filename.err"
rm "/tmp/$filename.bin"

# echo "directory = $directory; template = $template; filename = $filename;"

