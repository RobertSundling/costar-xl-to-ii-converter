# costar-xl-to-ii-converter

![GitHub License](https://img.shields.io/github/license/RobertSundling/costar-xl-to-ii-converter)

Vintage utility to convert raw CoStar LabelWriter XL printer data to LabelWriter II Plus format. Written by Robert Sundling in 2001.

## Overview and Motivation

This command-line tool was written to bridge the gap between a CoStar LabelWriter XL driver and an old LabelWriter II Plus printer, for which no modern Windows driver was available.

I used my LabelWriter II Plus printer for printing shipping labels for my shareware games, but when I upgraded to newer systems I found the LabelWriter II Plus was no longer supported by the CoStar LabelWriter drivers. Rather than buying a new printer, since mine was physically working fine, I decided the simplest way to get things working again was to just write a quick-and-dirty utility that would convert the output from the current driver to a format that the older printer could understand.

It receives the raw printer data from the CoStar LabelWriter XL printer driver then converts it to a compatible format for the older printer (including scaling the bitmap images, since the resolution of the II Plus was lower than the XL). The output can then be sent to a LabelWriter II Plus printer.

This allowed me to continue to use my old printer with the new drivers.

At the time [Ghostscript](https://en.wikipedia.org/wiki/Ghostscript) included a utility, [RedMon](http://www.ghostgum.com.au/software/redmon.htm), which creates a virtual printer port that can redirect output to any program. I used it to create virtual printer port on my Windows NT 4 server, setting it to redirect to this utility then pipe its output to the actual printer. I configured the CoStar LabelWriter XL driver, from my client computer, to print to that RedMon virtual port on the server. From there, everything worked automatically.

## Technical Details

This code was written in C++ for [Visual C++ 6.0](https://winworldpc.com/product/visual-c/6x). It wasn't written with the intent of being shared, but just as something to quickly solve an immediate problem. I haven't modified it since I got it working in 2001. You will find lots of commented-out debugging code that I used during development to test things. Once it worked, I didn't go back and clean it up.

For completeness, I have also included the original project and workspace files.

This code would likely be trivially modified to work with modern C++ compilers or other operating systems. I'd imagine that today this code would be primarily useful to study by those who want to print to a LabelWriter II Plus printer, or who want to decode data from a CoStar LabelWriter XL driver, not for anyone who actually wants to convert between the two.

The resampling code (from 203 to 136 dpi) is not likely useful. It's written in a weidly inefficient way because I was experimenting with different techniques--such as ORing pixels to ensure black ones wouldn't be missed--but ended up finding the best quality was had simply by choosing thicker fonts and resampling naïvely.

## Usage

Compile this, then using RedMon (or a similar utility), redirect the output from a LabelWriter XL driver into this program, and pipe its output to a LabelWriter II Plus printer.

## License

This project is available under the MIT License. See the LICENSE file for details.

## Contributing

As this is an archival project, contributions are not being sought and pull requests will not be accepted. However, feel free to fork the repository and make modifications for personal use.
