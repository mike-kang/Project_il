#!/usr/bin/python

fifo_write = open('/tmp/cmd', 'w')
fifo_write.write("memory\n")
fifo_write.flush()
fifo_read = open('/tmp/response', 'r')
respond = fifo_read.read()
print respond