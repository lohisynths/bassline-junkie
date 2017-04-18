#while [ 1 ]
#do
rm *.bin 
rm *.png
sudo ./x86_debug/bassline-junkie 
sudo chown alax:alax *.bin
python plot.py
#done

