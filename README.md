# Use TCP Socket to implement a chatroom on the mininet。

## Server: <br/>
**Usage:** ./server &lt;port number&gt;
1. Use multi-thread to handle requests from clients.
2. List all the members and chat room online, client can choose which room to join.
3. Handle clients request:
* Send messages to the member who in the same group。
* Decide person or group to receive the messages 

## Client: <br/>
**Usage:** ./client &lt;Server IP&gt; &lt;Port number&gt;<br/>
Connect to server.<br/>

**Handle input:**
1. &lt;Message&gt; <br/>
Send the messages to the group <br/>
2. /W &lt;Name or room&gt; &lt;Message&gt; <br/>
Decide person or group to receive the messages <br/>
3. Bye <br/>
Disconnection <br/>

**Example enviroment:** <br/>
Construct 4 host on the mininet and they can “ping” each other. <br/>
h1 is server <br/>
h2 h3 h4 is client <br/>
![image](https://github.com/dlgs5100/Chatroom_Linux_Socket/blob/master/image/2.PNG) <br/>
![image](https://github.com/dlgs5100/Chatroom_Linux_Socket/blob/master/image/1.jpg) <br/>
