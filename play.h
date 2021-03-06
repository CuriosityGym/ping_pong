const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
    <head>
      <meta charset="utf-8">
      <title>Forever alone ping pong</title>
      <style media="screen">
        html {
          background-color:#000000;
        }
        #score {
          text-align: center;
          font-size: 4em;
          margin:0px 0px 0px 0px;
          font-family: sans-serif;
          color:white;
          
        }
        #score.game-over {
          font-size:4em;
        }
        #message {
          display:none;
          text-align: center;
          font-size: 4em;
          font-family: sans-serif;
          color:white;                    
          text-decoration: blink;
        }
        #message.active, #score.game-over {
          display:block;
          animation: blink-animation 1.5s steps(5, start) infinite;
          -webkit-animation: blink-animation 1.5s steps(5, start) infinite;
          margin:0px;
        }
        @keyframes blink-animation {
          to {
            visibility: hidden;
          }
        }
        @-webkit-keyframes blink-animation {
          to {
            visibility: hidden;
          }
        }
      </style>
    </head>
    <body>
      <h1 id="score">0</h1>
      <h2 id="message">GAME OVER</h2>
      <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
      <script type="text/javascript">
      var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
      connection.onmessage = function (e) {
        if (e.data === "Game over") {
           $("#score").addClass("game-over");
           $("#message").addClass("active");
        } else {
           $("#message").removeClass("active");
           $("#score").removeClass("game-over");
           $("#score").html(e.data);
         }
      };
      </script>
    </body>
  </html>
)=====";
