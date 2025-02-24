# Define the serial port and baud rate
serial_port="/dev/ttyUSB0"
serial_port_2="/dev/ttyUSB2"
baud_rate="9600"

file_path  ="RR-Logos-BIG.txt"
file_path_2="RR-Logos-BIG-R.txt"
file_path_3="StarWarsRetroRecipes.txt"

# Delay (in seconds) between characters & Lines
delay_between_chars=0.0009
delay_between_lines=0.0009
delay_between_pages=3.5
lines_per_page=12
countLine=0 

# Configure the serial port settings
`stty -F "#{serial_port}" "#{baud_rate}" cs8 -cstopb -parenb`
`stty -F "#{serial_port_2}" "#{baud_rate}" cs8 -cstopb -parenb`

# Send the same file contents over and over forever
buf = []
bufg = []
bufh = []


# read files in
File.open(file_path) do |f|
  while(!f.eof?) do
    buf << f.gets.chomp
  end
end
File.open(file_path_2) do |g|
  while(!g.eof?) do
    bufg << g.gets.chomp
  end
end
File.open(file_path_3) do |h|
  while(!h.eof?) do
    bufh << h.gets.chomp
  end
end

fd = File.open(serial_port,"w")
fe = File.open(serial_port_2,"w")

while true do

  buf.each do |b|
    fd.print("\r\n")
    fe.print("\r\n")
    b.each_char do |c|
      sleep(delay_between_chars)
      fd.print c
      fe.print c
    end
    sleep(delay_between_lines)
    fd.flush
    fe.flush
  end
  sleep(delay_between_pages)

  bufh.each do |b|
    fd.print("\r\n")
    fe.print("\r\n")
    b.each_char do |c|
      sleep(delay_between_chars)
      fd.print c
      fe.print c
    end
    sleep(delay_between_lines)
    fd.flush
    fe.flush
  end
  sleep(delay_between_pages)

  bufg.each do |b|
    fd.print("\r\n")
    fe.print("\r\n")
    b.each_char do |c|
      sleep(delay_between_chars)
      fd.print c
      fe.print c
    end
    sleep(delay_between_lines)
    fd.flush
    fe.flush
  end
  sleep(delay_between_pages)

  bufh.each do |b|
    fd.print("\r\n")
    fe.print("\r\n")
    b.each_char do |c|
      sleep(delay_between_chars)
      fd.print c
      fe.print c
    end
    sleep(delay_between_lines)
    fd.flush
    fe.flush
  end
  sleep(delay_between_pages)

end

# Clean up
stty sane < "$serial_port"



