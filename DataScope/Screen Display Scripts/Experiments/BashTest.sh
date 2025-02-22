delay_between_pages="2.0"
ScreenLines=12
counter=0

do
  echo "Iteration: $counter"
  ((counter++))

done while [ $counter -le $ScreenLines ]
sleep "$delay_between_pages"
counter=0