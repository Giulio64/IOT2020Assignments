import React, { Component } from 'react';
import { PersistanceStorage } from "./PersistanceStorage/PersistanceStorage"
import LogTables from "./Component/LogsTable"
import wrprLogData from "./Wrapper/wrapperLogData";

import logo from './logo.svg';
import './App.css';

class App extends Component {

  state = { 
    loading :true,
    StationTableData:{},
    SensorTableData:{}
  }

  render(){
    return (
      <div className="App">
        {this.createNavBar()}
        <div className="container-md">
        {this.createStationTable()}
        {this.createSensorTable()}
        </div>
      </div>
    );
  }

  UNSAFE_componentWillMount(){
    let storage = new PersistanceStorage();
    storage.getLogs().then(data => {
    
      const wrapper = new wrprLogData.wrapperLogData(data);

      this.setState({
        loading: false,
        StationTableData: wrapper.getDataOrderedByStation(),
        SensorTableData: wrapper.getDataOrderedBySensor()
      })
    }).catch(error => {
      console.log(error.message);
      alert(error.message);
    });
  }

/**
 * Create the navigation bar 
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
  createNavBar(){
  return(
  <nav className="navbar navbar-expand-lg navbar-light" style={{boxShadow: "5px 10px 18px #888888",background:"#ac0d20"}}>
  <a className="navbar-brand" href="#" style={{fontWeight:"bold",color:"white"}}>Acme Wheather System</a>
  </nav>
  );
}

/**
 * Create the table where the logs are ordered by weather station
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
createStationTable(){

  return(
  <div style={{borderRadius:"25px" ,boxShadow: "5px 10px 18px #888888",align:"center",
  marginTop:"50px",paddingTop:"25px",paddingBottom:"25px"}}>

  <text style={{marginBottom:"40px",color:"#ac0d20",fontWeight:"bold"}}>Telemetry by Weather Station</text>
  {this.state.loading ? this.createSpinner() : <LogTables value={this.state.StationTableData}/>}
  <text style={{marginBottom:"40px",color:"#ac0d20",fontWeight:"bold",fontSize:"small"}}>Data limited to today</text>
  </div>

  );
}

/**
 * Create the table where the logs are ordered by sensor
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
 createSensorTable(){
  return(
    <div style={{borderRadius:"25px" ,boxShadow: "5px 10px 18px #888888",align:"center",
    marginTop:"50px",paddingTop:"25px",paddingBottom:"25px", marginBottom:"50px"}}>
  
    <text style={{marginBottom:"40px",color:"#ac0d20",fontWeight:"bold"}}>Telemetry by Sensor</text>
   
    {this.state.loading ? this.createSpinner() : <LogTables value={this.state.SensorTableData}/>}
    <text style={{marginBottom:"40px",color:"#ac0d20",fontWeight:"bold",fontSize:"small"}}>Data limited to the last hour</text>
    </div>
    );
}

/**
 * Create a loading spinner
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
 createSpinner(){
  return(
    <div style={{marginTop:"10px",display:this.state.loading}}>
    <div className="spinner-border spinner-border text-danger" role="status">
      <span className="sr-only">Loading...</span>
    </div>
    </div>
  );
}

}

export default App;
