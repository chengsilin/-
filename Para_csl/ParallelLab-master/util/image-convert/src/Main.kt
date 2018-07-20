import java.awt.image.BufferedImage
import java.io.DataInputStream
import java.io.DataOutputStream
import java.io.File
import javax.imageio.ImageIO


const val Threshold = 128

fun main(args: Array<String>) {
    require(args.size == 3, { "Usage: imgcvt <mode> <input file> <output file>\n" +
        "mode: b (image to binary) or p (binary to png)" })

    val binary = when (args[0]) {
        "b" -> true
        "p" -> false
        else -> error("Invalid mode.")
    }
    val input = File(args[1])
    val output = File(args[2])

    require(input.isFile, { "Input must be a file" })

    if (binary) {
        val img = ImageIO.read(input)

        println("Image size (${img.width}, ${img.height}), threshold $Threshold, converting...")

        DataOutputStream(output.outputStream()).use { w ->
            w.writeInt(img.width)
            w.writeInt(img.height)

            val bytes = img.getRGB(0, 0, img.width, img.height, null, 0, img.width)
                .map { raw ->
                    val r = (raw shr 24) and 0xFF
                    val g = (raw shr 16) and 0xFF
                    val b = (raw shr 8) and 0xFF

                    val avg = (r + g + b) / 3
                    if (avg < Threshold) 0.toByte() else 1.toByte()
                }
                .toByteArray()
            w.write(bytes)
        }

        println("Binary file written to $output.")
    } else {
        DataInputStream(input.inputStream()).use { w ->
            val width = w.readInt()
            val height = w.readInt()
            val bytes = w.readBytes(width * height)
            val rgbArray = bytes.map { (if (it == 0.toByte()) 0xFF000000 else 0xFFFFFFFF).toInt() }.toIntArray()

            val img = BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB)
            img.setRGB(0, 0, width, height, rgbArray, 0, width)

            ImageIO.write(img, "png", output)
        }

        println("PNG file written to $output.")
    }
}