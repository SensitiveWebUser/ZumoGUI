import { useContext } from "react";
import Gamepad from "react-gamepad";

import { Controller } from "./controller";
import { SocketContext } from "./context/socket";

const App = (props) => {
  const socket = useContext(SocketContext);

  return (
    <div className="app-root">
      <h1>Serial Data</h1>
      <p>Click the buttons to send data to the serial port</p>
      <Gamepad
        onA={() => socket.emit("serialdata", "s")}
        onB={() => socket.emit("serialdata", "d")}
        onX={() => socket.emit("serialdata", "a")}
        onY={() => socket.emit("serialdata", "w")}
        //
        onLB={() => socket.emit("serialdata", "z")}
        //
        onLT={() => socket.emit("serialdata", "q")}
        onRT={() => socket.emit("serialdata", "e")}
        //
        onUp={() => socket.emit("serialdata", "w")}
        onDown={() => socket.emit("serialdata", "s")}
        onLeft={() => socket.emit("serialdata", "a")}
        onRight={() => socket.emit("serialdata", "d")}
        //
        onRB={() => socket.emit("serialdata", "q")}
        onLS={() => socket.emit("serialdata", "e")}
      >
        <Controller />
      </Gamepad>
    </div>
  );
};

export default App;
