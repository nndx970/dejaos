import { Button, Render, Text, View } from "lvgljs-ui";
import React from "react";
import "./test/gpio.js"
import "./test/net.js"

function App() {
  return (
    <View style={{ "background-color": "blue" }}>
      <Button style={{ "background-color": "red" }}>
        <Text>Hello</Text>
      </Button>
    </View>
  );
}

Render.render(<App />);
