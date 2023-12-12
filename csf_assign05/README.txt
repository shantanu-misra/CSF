Sample README.txt

Contributions:
Shantanu: Connection, sender, receiver, message_queue, room, server
Sebastian: receiver, client_utils, server, room, README

Eventually your report about how you implemented thread synchronization
in the server should go here

There are varies moments in the code where we needed to be very careful with thread synchrinization.
We handled most of these critical sections by using guard objects that would make sure
only one thread has access to that part of the code at one time, therefore allowing
concurrency with our data structures used.

For example, for our message_queue, we used a guard object whenever the queue needs to be modified as these queues must be
accessed one at a time in order for the data structure to keep its atomicity. We also used semaphores in this implamentation in
order to notify any thread waiting that that resource is available.

More Guard objects were used all over the implamentation of the Room class as we also need to protect the memberSet object.
This object is a set containing all of the members of a room and it can be modified by users. Therefore we must make sure that at 
the time of its call by a specific client, another clients call will not affect it and disrupt that data structure by removing or adding
a user to it. We also initialized a mutex when creating the room object and destroyed it in its constructor as it should be done.

Furthermore, In the server class, we had to make sure that the RoomMap would be concurrent in all executations. This had to be handled
in the find_or_create_room method, and we used a guard in that method to gurantee that that critical section could only be accessed by one thread at a time
in order to keep its atomicity.

In conclusion, No deadlocks should occur in this program as there should be no point where a thread is left waiting for permision to access some data, as the guard object
is used throughout, which provides an robust way to format the sections of the code where a thread would have to wait for access.