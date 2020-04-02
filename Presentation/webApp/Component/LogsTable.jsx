import React, { Component } from "react";

class LogsTable extends Component {

  constructor(props) {
    super(props);
    this.state = {
        optionToDisplay:Object.keys(this.props.value)[0]
    };    
  }

  render() {
    return this.createTable();
  }

  entrySelected = (e) => {
      this.setState({
        optionToDisplay:e.target.value
      })
  };

  /**
   * Create a generic table with a selector for data to display
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   */
  createTable() {
    return (
      <div>
        <center>
          <select
            className="form-control"
            onChange={e => this.entrySelected(e)}
            style={{
              marginTop: "20px",
              marginBottom: "20px",
              marginLeft: "20px",
              marginRight: "20px",
              width: "70%"
            }}
          >
            {
              Object.keys(this.props.value).map((key, value) => (
                this.createOption(key)
              ))
            }
          </select>
        </center>
        <table
          className="table"
          style={{ objectFit: "cover", marginTop: "30px" }}
        >
          <thead>
            <tr>
              <th scope="col">DateTime</th>
              <th scope="col">Sensor Name</th>
              <th scope="col">Sensor Type</th>
              <th scope="col">Value</th>
              <th scope="col">Station</th>
            </tr>
          </thead>
          <tbody>
          {
             this.createRows()
          }
          </tbody>
        </table>
      </div>
    );
  }

  /**
   * Create all the rows to display in the table
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   * @param text [Wrapper containing data to display in a row]
   */
  createRows(){

    console.log({tableopt:this.props.value})
    var rowsList = [];
    for(const option in this.props.value){
      var logsForOption = this.props.value[option];

      if(option === this.state.optionToDisplay){
        for(const index in logsForOption){
          const log = logsForOption[index];
          rowsList.push(this.createRow(log));
        }
      }
    }

    return rowsList;
  }

  /**
   * Create an option value for the select componenet
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   * @param text [Wrapper containing data to display in a row]
   */
  createOption(text){
    return(
    <option>{text}</option>);
  }

  /**
   * Create a single row for the table
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   * @param wrapper [Wrapper containing data to display in a row]
   */
  createRow(wrapper){
    return( 
      <tr>
      <th scope="row">{wrapper.date}</th>
      <td>{wrapper.sensorName}</td>
      <td>{wrapper.sensorType}</td>
      <td>{wrapper.value}</td>
      <td>{wrapper.station}</td>
    </tr>)
  }
}

export default LogsTable;
