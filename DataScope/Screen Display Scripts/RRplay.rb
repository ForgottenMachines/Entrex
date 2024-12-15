# Define the serial port and baud rate
serial_port="/dev/ttyUSB0"
baud_rate="115200"
#baud_rate="9600"

file_path="RR Logos.txt"

# Delay (in seconds) between characters & Lines
delay_between_chars=0.05
delay_between_lines=0.05
delay_between_pages=8.0
lines_per_page=12
countLine=0 

# Configure the serial port settings
`stty -F "#{serial_port}" "#{baud_rate}" cs8 -cstopb -parenb`

# Send the same file contents over and over forever
buf = []
# read file in
File.open(file_path) do |f|
  while(!f.eof?) do
    buf << f.gets.chomp
  end
end
fd = File.open("/dev/ttyUSB0","w")

while true do

buf.each do |b|
     b.each_char do |c|
        sleep(delay_between_chars)
        fd.print c
  end
    sleep(delay_between_lines)
    fd.print("\r")
    fd.flush
    countLine = countLine + 1
    puts countLine
    if countLine=lines_per_page 
    #    sleep(delay_between_pages)
        countLine = 0
      end
      
end

#  fd.puts("Yours in Retro...")
  sleep(10)
end

# Clean up
stty sane < "$serial_port"



