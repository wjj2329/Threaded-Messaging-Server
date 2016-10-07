# Lab 2: Threaded Messaging Service

## William Jones & Jonathan Dayton

We opted to exclusively use mutexes and condition variables instead of writing threadsafe classes.  

We have two shared variables: the client queue and the message map.  We also have two mutexes that protect every access of either of those two variables respectively.  We have a third mutex for initializing the unique_lock that is used by the condition variable.  

The condition variable is used twice in our code.  First, it makes the threads wait if the client queue is empty.  Second, every time the client queue is pushed to, one thread is notified to wake up and handle the new client.  
