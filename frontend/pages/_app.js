import "../styles/globals.css";
import * as React from "react";
import Layout from "../components/layout/Layout";
import Map from "../components/generic/Map";
import { GlobalContextProvider } from "./store/globalContext";
import { NextUIProvider } from "@nextui-org/react";

function MyApp() {
  return (
    <NextUIProvider>
      <GlobalContextProvider>
        <Layout>
          <Map />
        </Layout>
      </GlobalContextProvider>
    </NextUIProvider>
  );
}
export default MyApp;
