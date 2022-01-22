base_site="https://d3o4twxzdiwvsf.cloudfront.net/fecnetwork/hdtimes10.flv/media-ub76glkio_"
link_num=2282

rm -r temp
mkdir temp
cd temp
touch list.txt
echo "" > list.txt
for i in {0..25}
  do
    file=$link_num".mp4"
    curl $base_site$link_num".ts" --output $file
    echo "file "$file >> list.txt
    link_num=$((link_num+1))
done

ffmpeg -f concat -safe 0 -i list.txt -c copy output3.mp4
cd ../