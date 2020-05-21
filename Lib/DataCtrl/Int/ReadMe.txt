==================================================================================================
       DYNAMIC LINK LIBRARY : DataCtrl
==================================================================================================

Guideline - Using DLL as a server
=================================
Startup
1. Start(Port No, Interface IP)
2. SetSvrDPID(DP ID, DP Instance ID)
3. AddChannel(MCast IP, Port No)
4. Repeat 3 if required.
5. AddMonitorChannel(MCast IP, Port No)
   This function must be called once only and it will add more checking in Arbitrator.
   The standby servers will listen to the specified MCast Group.  If the standby servers cannot recieved
   any Multicast Data within 3 to 4 seconds from the Primary server, the standby servers will ignore
   the primary and start to sending out the raise hand tokens.
6. Send(COP item)

Shutdown
1. DelMonitorChannel(MCast IP, Port No)
2. DelChannel(MCast IP, Port No)
3. Repeat 2 if required.
4. Stop()


Registry Parameter Description
==============================
Location: [HKEY_LOCAL_MACHINE\SOFTWARE\AFE\DataCtrl]

                       Self Create
Key                    with Default    Description
---                    ------------    -----------
LogEnable              0x00000000      Dump the errorness packet whenever Datactrl failed to 
                                       validate a packet. The log file is "datactrl.log".

PacketLogEnable        NO DEFAULT      Create log file of each pack with sequence number and packet
                                       size in hourly bases. The hourly log files are 
                                       "packet_mmdd_hh.log".
                                       It also create per second statistics bit per second with 336
                                       bit assumed packet overhead. packet per second. byte per
                                       second. The log files are daily basis "BPS_mmdd.log"

ReceiveBufferEnable    0x00000001      DataCtrl contains a internal thread and buffer to receive 
                                       and buffer the multicast message. Since DataCtrl has a 
                                       separate thread to handle incomingtraffic, it can minimize 
                                       packet loss due to DataCtrl message processing thread being
                                       hold up by the application during call back.

RecoveryWindowSize     0x0000000b      Each packet from Tx send with a sequence number in page
                                       0xFE000000 fid 263. DataCtrl would handle packets comes in 
                                       wrong order of sequence, or duplicate packet.if it is within 
                                       the recovery window size. It is set to 11 by default.

                                       Receive Duplicate Packet
                                       ------------------------
                                       - If <received packet seq> equals <expected seq - 1>
                                       - If <received packet seq> less than <expected seq - 1> but 
                                         within the window size of the <expected seq>
                                       - If <received packet seq> equals one of the <packet seq> kept
                                         in the recovery window
                                       Action:
                                       * The <received packet> will be ignored

                                       Received Packet Out Of Sequence
                                       -------------------------------
                                       - If <received packet seq> greater <expected seq>
                                       - If <received packet seq> less than <expected seq> and outside
                                         the window size of the <expected packet seq - 1>
                                       Action:
                                       * <received packet> will keep in recovery window with TTL set to
                                         recovery recovery window size. TTL of each packet keeps in the
                                         recovery window would decrement whenever a packet successfully
                                         pass to the application for processing.

                                       Recovery of Expected Packet
                                       ---------------------------
                                       - If <received packet seq> equals <expected seq> and there are
                                         pending packets hold in recovery windows
                                       Action:
                                       * <received packet> treated as recovery packet and get pending
                                         packets from the recovery window with expected sequence
                                         
                                       Expected Packet is Loss
                                       -----------------------
                                       - If <expected packet> does not arrive until the recovery windows
                                         filled up with out of seq packets.
                                       Action:
                                       * <expected packet> assumed loss. Next <expected packet> would
                                         base on the majority in sequence packets hold in recovery window. 
                                         Packets will get from the recovery windows in sequence until 
                                         sequence breaks.

SeqLogEnable           0x00000000      Create log file to note the event of recovery due to out of
                                       sequence, duplicate packet, packet loss. The log files are
                                       daily log "dctrl_seq_mmdd.log"

ThreadPriority         0c00000001      This parameter control the thread priority of the DataCtrl
                                       threads. By default it is set to THREAD_PRIORITY_ABOVE_NORMAL
                                       Valid priority are
                                       THREAD_PRIORITY_NORMAL          0
                                       THREAD_PRIORITY_ABOVE_NORMAL    1
                                       THREAD_PRIORITY_HIGHEST         2

Version                x.x.x.x         Current DataCtrl.dll version



Memeory Leak in Debug Mode for Datactrl.dll
===========================================

Detected memory leaks!
Dumping objects ->
{44} normal block at 0x003125D8, 33 bytes long.
 Data: < C              > 00 43 00 CD CD CD CD CD CD CD CD CD CD CD CD CD 
{43} normal block at 0x00312580, 40 bytes long.
 Data: < |L             > 14 7C 4C 10 16 00 00 00 00 00 00 00 00 00 00 00 
Object dump complete.

MSDN - PRB_ Memory Leaks Reported When Using Multiple MFC DLLs (Q167929)
Conclusion - Both Memory blocks {43} & {44} allocated by ...\VC98\CRT\SRC\CRTDLL.C Ln267
			 These memory leak notifications should be false, and can be ignored.  


==============
Change History
==============

Versions    Author      Date            Description
1.0.3.11    John Lee    15 Aug 2001     - Disable recovery process if the first message of a packet 
                                          is not a system messsage (0xFE000000) with FID 263.
                                        - Log "Recovery disable!" in datactrl.log if the recovery
                                          process is disabled.
1.0.3.12    John Lee    17 Aug 2001     - Support multiple DP ID and Ins for recovery process.
                                        - Support multiple register callback and unregister for
                                          disabled COP buffer.
1.0.3.13    John Lee    17 Aug 2001     - Log message int datactrl.log while disabling receovery
                                          process.
1.0.3.14    John Lee    20 Aug 2001     - Return 1 for AddForce function if toal size > m_iLimit.

1.0.3.15    Terence Lai 24 Oct 2001     - Added ThreadPriority flag at registry to change the thread
                                          priority in data control. Default is set to ABOVE_NORMAL
1.0.3.16    John Lee    21 Dec 2001     - Use GUID to identidy the On-demand-rebuild request and
                                          change new API for front end.
1.0.4.17    John Lee    22 Feb 2002     - Add VirtualReset for message.cpp & .h
                                          (The change is especially for TX only)
1.0.5.18    John Lee    11 Mar 2002     - Open log file in text mode instead of binary mode.
                                        - Can set the recovery window size in registry.
                                        - log recovery packet size.
1.0.6.19    John Lee    14 Mar 2002     - log recovery not-in-queue packet size.
1.0.6.20    Terence Lai 14 Mar 2002     - Add registry parameter "EnablePacketLog" to log all receive
                                          packet with packet sequence number and packet size 
                                          (exclude 336bit over head)
                                          This new parameter would not create in registry by default
1.0.6.21    Terence     11 Apr 2002     - Split packet.log into hourly log
                                        - Split dctrl_seq.log into daily log
1.0.7.22    John Lee    15 Apr 2002     - Ignore recovery item if the seq no of the item < current 
                                          seq no and not within window size.
                                        - Add TTL to recovery queue item.  In initialization, TTL =
                                          window size.  TTL decreases one for a in-sequence packet.
                                          Remove recovery item if its TTL = 0.
                                        - Add critical section while accessing callback function
                                          pointer.
1.0.8.23    John Lee    17 Apr 2002     - GetMsgLen() from COP first before calling ProcessCOP() 
                                          in CRecvCtrl.
1.0.8.24    Terence Lai 19 Apr 2002     - Test message object lenght before and after processCOP 
                                          produce log
1.0.9.25    John Lee    19 Apr 2002     - Bug Fix: using wrong variable to process the TTL.
1.0.10.26   John Lee    23 Apr 2002     - Enable COP Active Buffer by default.
1.0.10.27   Terence Lai 03 May 2002     - Add interface DataCtrl::GetCliMaxCount() to get the peak no. of 
                                          items in receive buffer 
1.0.10.28   Terence Lai 07 May 2002     - Add log on bit per second with assumption of 336 overhead 
2.0.1.29	John Lee	31 May 2002		- Add AddMonitorChannel() & DelMonitorChannel() to check
										  MCast Data.  It will stop to recieve UDP packet from the
										  current Primary server if no multicast data within 3 seconds
										  and start to raise hand.
										- Add thread for data recieving statistics.
(Tx v2.0.1.42 must use Datactrl.dll v2.0.1.29 or above)
2.0.2.30	John Lee	5 Jul 2002		- Bug Fix: Add handling on recovery window size = 0.
2.0.2.31    John Lee    20 Sep 2002     - Build with new library.
                                          MsgObject.lib v1.1.1.24
2.0.3.35    John Lee    18 Oct 2004     - Bug Fix: change DP ID storage from byte to short.
2.1.0.36	John Lee	4 Jan 2007		- Add SetIncrement(), SetMaxBound(), SetMaxLimit() for RecvCtrl
2.2.0.37	John Lee	2 Jan 2009		- Bug Fix: SetSvrDPID uses unsigned short instead of byte
2.2.1.38	John Lee	20 Oct 2010		- Add OOB queue for system message
2.2.2.39	John Lee	10 Feb 2011		- Add Message type checking and show error log with remote IP

///////////////////////////////////////////////////////////////////////////////////////////////////
