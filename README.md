# HA_YEDIK <br />
The Goal <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;To create a simple lightweight client dependent file sharing system using P2P networking. <br />
Description <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The project uses light socket wrapper and WTL, a header-only library that extends ATL and wraps Win32 (it has little to no extra overhead). <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The Logic <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;A central tracker that launches trackers on seperate threads, on demand - <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The Central tracker receives an empty packet from a client, launches a tracker, and that tracker sends an empty packet back. If everything goes well the client will store the <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;new tracker's port (and presumably share it) and send an empty packet back for confirmation, otherwise the tracker will be lost. The tracker will then store a list of clients that a new client can aquire. <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;A client that acts as both client and server - <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;A client that aquires such a list can then request the file's data (name and size), and afterwards the file itself, piece by piece, from all of the clients. <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;After the client finishes downloading, it sends a message to the tracker stating that it (the client) has finished downloading, and is now free to share the file. <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Therefore, the tracker adds the client's address to the list, and a new client that wants to download the file will be able to download file data and pieces of the file from the previously mentioned client. <br />