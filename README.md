# Game Server for 'Čas za obračun'



Test UDP packet se poslje s komando (netcat)<br>
`nc -u 127.0.0.1 42069 <<< "message"`

sintaksa:<br>
`nc -u <server-ip> <target-port> <<< "msg"`

<br>
Testiranje v powershell:<br> 

```PS
$udpClient = New-Object System.Net.Sockets.UdpClient
$serverIp = "192.168.64.110"
$port = 42069
$message = "Pozdravljen server!"
$bytes = [System.Text.Encoding]::UTF8.GetBytes($message)
$udpClient.Send($bytes, $bytes.Length, $serverIp, $port)
$udpClient.Close()
```

Zaenkrat je treba killat task da ga ugasneš:<br>
> `kill -9 PID`