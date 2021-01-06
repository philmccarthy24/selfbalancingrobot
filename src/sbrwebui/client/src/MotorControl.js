import React from 'react';
import MeasureAndRender from './MeasureAndRender';

let MotorControl = () => {
    
    /*
    <svg>
                    <line x1={this.clientRect.left + this.clientRect.width/2} y1={this.clientRect.top} x2={this.clientRect.left + this.clientRect.width/2} y2={this.clientRect.bottom} stroke="black" strokeWidth="1" />
                    <line x1={this.clientRect.left} y1={this.clientRect.top + this.clientRect.height/2} x2={this.clientRect.right} y2={this.clientRect.top + this.clientRect.height/2} stroke="black" strokeWidth="1" />
                </svg>
    */

        return (
            <MeasureAndRender stretch={true} debounce={1}>
                {bounds => {
                    return (<div>
                        <h2>MotorControl placeholder {bounds.width}</h2>
                        <svg>
                            <line x1={bounds.left + bounds.width/2} y1={bounds.top} x2={bounds.left + bounds.width/2} y2={bounds.bottom} stroke="black" strokeWidth="1" />
                            <line x1={bounds.left} y1={bounds.top + bounds.height/2} x2={bounds.right} y2={bounds.top + bounds.height/2} stroke="black" strokeWidth="1" />
                            <circle cx={50} cy={50} r={10} fill="blue" />
                        </svg>
                    </div>
                    );
                }}
            </MeasureAndRender>
        );
  }

export default MotorControl;