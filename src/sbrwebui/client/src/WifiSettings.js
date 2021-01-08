import React from 'react';
import { TextField, FormControlLabel, Switch, Paper } from '@material-ui/core';

function WifiSettings(props) {
  const [state, setState] = React.useState({
    checkedA: true,
    checkedB: true,
  });

  const handleChange = (event) => {
    setState({ ...state, [event.target.name]: event.target.checked });
  };

    return (
      <div className="WifiSettings">
          <h2>WifiSettings placeholder</h2>
          <form>
            <Paper>
            <TextField id="siid" label="SIID" defaultValue="foo" /><br/>
            <TextField id="siid_pw" label="Password" defaultValue="foo" password /><br/>
            <FormControlLabel
        control={
          <Switch
            checked={state.checkedB}
            onChange={handleChange}
            name="checkedB"
            color="primary"
          />
        }
        label="Enable as access point"
      />
      </Paper>
      <FormControlLabel
        control={
          <Switch
            checked={state.checkedB}
            onChange={handleChange}
            name="checkedB"
            color="primary"
          />
        }
        label="Use DHCP"
      />
          </form>
          

          
      </div>
    );
  }

export default WifiSettings;