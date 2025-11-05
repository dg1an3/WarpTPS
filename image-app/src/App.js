import React, { useState } from 'react';
import './App.css';

import ImageOpsContainer from './Container/ImageOps';
import TPSWarping from './Container/TPSWarping';
import AppBar from '@material-ui/core/AppBar';
import Tabs from '@material-ui/core/Tabs';
import Tab from '@material-ui/core/Tab';
import Box from '@material-ui/core/Box';

function TabPanel(props) {
  const { children, value, index, ...other } = props;

  return (
    <div
      role="tabpanel"
      hidden={value !== index}
      id={`tabpanel-${index}`}
      aria-labelledby={`tab-${index}`}
      {...other}
    >
      {value === index && (
        <Box p={3}>
          {children}
        </Box>
      )}
    </div>
  );
}

function App() {
  const [tabValue, setTabValue] = useState(0);

  const handleTabChange = (event, newValue) => {
    setTabValue(newValue);
  };

  return (
    <div className="App">
      <AppBar position="static">
        <Tabs value={tabValue} onChange={handleTabChange} aria-label="app tabs">
          <Tab label="TPS Warping" />
          <Tab label="Color Filters (Cloudinary)" />
        </Tabs>
      </AppBar>
      <TabPanel value={tabValue} index={0}>
        <TPSWarping />
      </TabPanel>
      <TabPanel value={tabValue} index={1}>
        <ImageOpsContainer />
      </TabPanel>
    </div>
  );
}

export default App;
