import React, { useEffect } from "react";
import Routes from "./Routes";

function App() {
  useEffect(() => {
    const savedTheme = localStorage.getItem("academy-theme");
    if (savedTheme) {
      document.documentElement.dataset.theme = savedTheme;
    }
  }, []);

  return <Routes />;
}

export default App;
