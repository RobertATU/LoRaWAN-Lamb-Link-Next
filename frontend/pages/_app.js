import '../styles/globals.css'
import Layout from '../components/layout/Layout'
import Map from '../components/generic/Map'
import { GlobalContextProvider } from './store/globalContext'


function MyApp({ Component, pageProps }) {
  return (
    <GlobalContextProvider>
      <Layout>
        <Map/>
      </Layout>
    </GlobalContextProvider>
  );
}

export default MyApp
