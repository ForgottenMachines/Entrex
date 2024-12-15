#!/bin/bash

# Define the serial port and baud rate
serial_port="/dev/ttyUSB0"
baud_rate="115200"
#baud_rate="9600"

file_path="RRLogos.txt"

# Delay (in seconds) between characters & Lines
delay_between_chars="0.001"
delay_between_lines="0.001"
delay_between_pages="2.0"


# Configure the serial port settings
stty -F "$serial_port" "$baud_rate" cs8 -cstopb -parenb

# Send the same file contents over and over forever
while true; do

    # Read the text file character by character and send it with a delay
    while IFS= read -r -d $'\n' line; do
        # Send the character over the serial port

        for ((i = 0; i < ${#line}; i++)); do
            char="${line:$i:1}"
            echo -n "$char" > "$serial_port"
            sleep "$delay_between_chars"
        done

        echo -n $'\r\n' > "$serial_port"
        sleep "$delay_between_lines"
    done < "$file_path"

done

# Clean up
stty sane < "$serial_port"



