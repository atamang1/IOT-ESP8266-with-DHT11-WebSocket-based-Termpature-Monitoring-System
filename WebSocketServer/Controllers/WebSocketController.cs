using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System.Collections.Concurrent;
using System.Net.WebSockets;
using System.Text;

namespace WebSocketServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class WebSocketController : ControllerBase
    {
        // A thread-safe collection to hold all connected WebSocket clients
        private static ConcurrentDictionary<string, WebSocket> _connectedClients = new ConcurrentDictionary<string, WebSocket>();

        // Endpoint for accepting WebSocket connections
        [HttpGet("/ws")] //Websocket path 'ws://localhost:port/ws
        public async Task Get() 
        {
            // Check if the incoming request is a WebSocket request
            if (HttpContext.WebSockets.IsWebSocketRequest)
            {
                var ipAddress = HttpContext.Connection.RemoteIpAddress?.ToString();
                var port = HttpContext.Connection.RemotePort;  // Gets the port number
                Console.WriteLine($"Client connected from: {ipAddress}:{port}");

                // Accept the WebSocket connection
                using var webSocket = await HttpContext.WebSockets.AcceptWebSocketAsync();

                //Store the valid Websocket connection in the dictrionary 
                // Client's IP address as the key
                if (ipAddress != null)
                {
                    _connectedClients[ipAddress] = webSocket;
                }

                // Start echoing messages from the WebSocket connection
                await Echo(webSocket, ipAddress);
            }
            else
            {
                // Return a 400 Bad Request if the connection is not a WebSocket request 
                HttpContext.Response.StatusCode = StatusCodes.Status400BadRequest;
            }
        }


        // Method to handle incoming messages and send responses 
        private static async Task Echo(WebSocket webSocket, string ipAddress)
        {
            // Buffer to hold the incoming message data
            var buffer = new byte[1024 * 4];
            WebSocketReceiveResult receiveResult;

            // Infinite loop to keep receiving messages from the WebSocket
            while (true)
            {
                // Receive the message from the WebSocket
                receiveResult = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);

                // If the WebSocket connection is closed, break out of the loop
                if (receiveResult.CloseStatus.HasValue)
                {
                    break;
                }

                // If the message is of type = 'Text', extract it
                if (receiveResult.MessageType == WebSocketMessageType.Text)
                {
                    // Convert the received buffer to a string message
                    var message = Encoding.UTF8.GetString(buffer, 0, receiveResult.Count);
                    Console.WriteLine($"Received message from {ipAddress}: {message}");

                    // Send a response message back to the sender (optional)
                    await webSocket.SendAsync(
                        new ArraySegment<byte>(Encoding.UTF8.GetBytes("Hello from the server!")),
                        WebSocketMessageType.Text,
                        true,
                        CancellationToken.None);

                    // Convert the received message to a byte array 
                    var bufferMessage = Encoding.UTF8.GetBytes(message);
                    var segment = new ArraySegment<byte>(bufferMessage);

                    // define target IP address 
                    var targetKey = "127.0.0.1"; //Localhost client's IP address 

                    // Check if the WebSocket for that IP and port exists 
                    if (_connectedClients.TryGetValue(targetKey, out var targetConnection))
                    {
                        Console.WriteLine(targetConnection);
                        // send the message to target,  if the WebSocket is open 
                        if (targetConnection.State == WebSocketState.Open)
                        {
                            await targetConnection.SendAsync(segment, WebSocketMessageType.Text, true, CancellationToken.None);
                        }
                    }
                }


            }

            // Close the WebSocket connection when done
            await webSocket.CloseAsync(
                receiveResult.CloseStatus.Value,
                receiveResult.CloseStatusDescription,
                CancellationToken.None);

            // Remove the WebSocket from the collection when the connection is closed
            _connectedClients.TryRemove(ipAddress, out webSocket);

        }



        // Broadcast a message to all connected clients 
        public static async Task BroadcastMessage(string message)
        {
            var buffer = Encoding.UTF8.GetBytes(message); 
            var segment = new ArraySegment<byte>(buffer);

            foreach (var webSocket in _connectedClients.Values)
            {
                if(webSocket.State == WebSocketState.Open)
                {
                    await webSocket.SendAsync(segment, WebSocketMessageType.Text, true, CancellationToken.None);
                }
            }
        }

    }
}
