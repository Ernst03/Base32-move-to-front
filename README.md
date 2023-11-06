
Ensure you have GCC and GMP installed on your system before compiling.

## Concept

This project is an illustration of invariant information. It utilizes a file that is a quintessential example of high entropy—“Mark Nelson’s Million Digit Challenge” ([Download Original File](https://marknelson.us/assets/2006-06-20-million-digit-challenge/AMillionRandomDigits.bin)).

When the binary data is expressed as a Base32 number, the `B32Mtf` codec applies the Move-to-Front (MTF) transformation iteratively on its textual digits. These characters are then converted into a new integer and through iteration of either ENCODE or DECODE generates a sequence of integers.
Both the "encode" and "decode" processes have been implemented and tested for up to fifty iterations. 

## Observations on Entropy

Using Gzip compression to evaluate the output, no significant compression was observed. This outcome suggests that merely reordering the elements of a high-entropy file does not necessarily reduce its entropy—that is, it doesn't bring a compressible structure to what was initially a random structure.

## Potential for Further Research

The `B32Mtf` program provides a foundation for those interested in exploring this phenomenon further. It serves as a scaffold for experimentation and is presented as open-source for community development and academic inquiry.

## Contributing

Contributions to B32Mtf are welcome. To contribute:

1. Fork the repository.
2. Create your feature branch (`git checkout -b feature/fooBar`).
3. Commit your changes (`git commit -am 'Add some fooBar'`).
4. Push to the branch (`git push origin feature/fooBar`).
5. Create a new Pull Request.

## License

This project is licensed under the GNU General Public License v3.0 - see the LICENSE.md file for details.

## Acknowledgments

- The original MTF algorithm creators: Bentley, Sleator, Tarjan, and Wei.
- The GNU Project and GMP Library.

## Author's Notes

B32Mtf originated from a simple idea and evolved with the assistance of artificial intelligence, marking a notable milestone in the field of information alchemy. It serves as a testament to the collaborative efforts between human curiosity and AI ingenuity.

