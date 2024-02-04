import React from 'react';
import { confirmAlert } from 'react-confirm-alert'; // Import
import 'react-confirm-alert/src/react-confirm-alert.css' // Import css

class Confirm extends React.Component {

  submit = () => {
    confirmAlert({
      title: 'Confirm to submit',
      message: 'Are you want to delete this pin.',
      buttons: [
        {
          label: 'Yes',
          onClick: () => alert('Click Yes')
        },
        {
          label: 'No',
          onClick: () => alert('Click No')
        }
      ]
    })
  };

  render() {
    return (
      <div className="container">
        <button onClick={this.submit}>Delete Pin</button>
      </div>
    );
  }
}
export default Confirm;