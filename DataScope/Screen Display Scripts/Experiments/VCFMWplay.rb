# Define the serial port and baud rate
serial_port="/dev/ttyUSB0"
baud_rate="115200"
#baud_rate="9600"

class FileData
  attr_reader :delay_chars
  attr_reader :delay_lines
  attr_reader :name

  def initialize(name, chars, lines)
    @name = name
    @delay_chars = chars
    @delay_lines = lines
  end
end

files=[
    FileData.new("DataScopeVCFMW_unix.txt", 0.001, 1.0),
]

# Configure the serial port settings
`stty -F "#{serial_port}" "#{baud_rate}" cs8 -cstopb -parenb`
fd = File.open("/dev/ttyUSB0","w")
#fd = File.open("Foo.txt","w")

files.each do |file|
  # Send the same file contents over and over forever
  buf = []

  # read file in
  File.open(file.name) do |f|
    while(!f.eof?) do
      buf << f.gets.chomp
    end
  end

  while true do
    buf.each do |b|
      b.each_char do |c|
        sleep(file.delay_chars)
        fd.print c
      end
      sleep(file.delay_lines)
      fd.print("\r")
      fd.flush
    end

    fd.puts("")
    sleep(3)
  end
end

# Clean up
stty sane < "$serial_port"
