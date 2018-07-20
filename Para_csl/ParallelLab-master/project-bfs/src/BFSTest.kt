import java.util.*
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.CopyOnWriteArrayList
import kotlin.concurrent.thread

val MaxDepth = 4
var BranchFactor = 40
val MaxThreads = 8

var count = 0

class Node(val value: Int, val children: Array<Node>)

fun incrCount(): Int {
    count += 1
    return count
}

fun getDeltaTime(t1: Long, t2: Long): Double {
    val dt = t2 - t1
    return dt.toDouble() / 1000000
}

fun populateNode(depth: Int): Node =
    if (depth == MaxDepth)
        Node(incrCount(), emptyArray())
    else
        Node(incrCount(), (0 until BranchFactor).map { populateNode(depth + 1) }.toTypedArray())

fun baselineTest(root: Node) {
    val t1 = System.nanoTime()

    val queue: Queue<Node> = ConcurrentLinkedQueue<Node>()
    queue.add(root)
    while (!queue.isEmpty()) {
        val next = queue.poll()
        next.children.forEach { queue.add(it) }
    }

    val t2 = System.nanoTime()

//    println(order.joinToString(separator = ",") { it.value.toString() })
    println("Baseline time " + getDeltaTime(t1, t2) + " ms")
}

fun multiThreadedTest(root: Node) {
    val t1 = System.nanoTime()

    val queue = ConcurrentLinkedQueue<Node>()
    queue.add(root)

    while (!queue.isEmpty()) {
        val curCount = queue.count()
        val threadCount = if (curCount <= MaxThreads) curCount else MaxThreads
        val perThread = (curCount + threadCount - 1) / threadCount
        (0 until threadCount).map {
            val x = perThread * it
            val toProcess = Math.min(perThread, curCount - x)
            thread {
                for (i in 0 until toProcess) {
                    val next = queue.poll()
                    next.children.forEach { queue.add(it) }
                }
            }
        }
        .forEach { it.join() }
    }

    val t2 = System.nanoTime()

    println("Threaded time " + getDeltaTime(t1, t2) + " ms")
}

fun testAll() {
    println("Populating nodes... BranchFactor=$BranchFactor")
    val root = populateNode(0)
    baselineTest(root)
    multiThreadedTest(root)
}

fun main(args: Array<String>) {
    BranchFactor = 40
    testAll()
}

