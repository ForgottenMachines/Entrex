# Define the serial port and baud rate
serial_port="/dev/ttyUSB0"
baud_rate="115200"
#baud_rate="9600"

file_path="DataScopeVCFMW_unix.txt"

# Delay (in seconds) between characters & Lines
delay_between_chars=0.001
delay_between_lines=1.0

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
end

  fd.puts("RESTARTING IN 10..")
  sleep(10)
end

# Clean up
stty sane < "$serial_port"



