import React from 'react';
import './MotorControl.css'

// code inspired by https://gist.github.com/hashrock/0e8f10d9a233127c5e33b09ca6883ff4
// TODO: encapsulate this using render props to allow any svg shape to be manipulated with mouse/touch, and allow
// x/y change events to be fired to parent component (that hosts the svg). NOTE not necessarily move, it could be resizing a vertical velocity bar.
// probably want to export a move event that gives pixel offset since last move handler call? don't know.
// scaling in this component feels a bit wrong... but I think we want changes in svg user space.
// see SVGDrag.js. Don't know how we get move x/y offset in svg space. will also need some way to convert to M0v and M1v.
const DraggableCircle = (props) => {
    const [position, setPosition] = React.useState({
      svgX: 0,
      svgY: 0,
      active: false,
      pxOffset: { }
    });
  
    const handlePointerDown = e => {
      const el = e.target;
      
      const bbox = e.target.getBoundingClientRect();
      const x = e.clientX - bbox.left;
      const y = e.clientY - bbox.top;
      // x y are the offset of the pointer-down-point from the top left corner of the bounding box, in pixels
      el.setPointerCapture(e.pointerId);
      setPosition({
        ...position,
        active: true,
        pxOffset: {
          x,
          y
        }
      });
    };
    const handlePointerMove = e => {
      const bbox = e.target.getBoundingClientRect();
      const xScale = props.svgElWidth / bbox.width;
      const yScale = props.svgElHeight / bbox.height;
      const x = e.clientX - bbox.left;
      const y = e.clientY - bbox.top;
      if (position.active) {
        setPosition({
          ...position,
          svgX: position.svgX + (xScale * (x - position.pxOffset.x)),
          svgY: position.svgY + (yScale * (y - position.pxOffset.y))
        });
      }
    };
    const handlePointerUp = e => {
      setPosition({
        ...position,
        active: false
      });
    };
  
    return (
      <circle
        cx={position.svgX}
        cy={position.svgY}
        r={0.1}
        onPointerDown={handlePointerDown}
        onPointerUp={handlePointerUp}
        onPointerMove={handlePointerMove}
        fill={position.active ? "#0000ffcc" : "#0000ff99"}
      />
    );
  };

class MotorControl extends React.Component {

    // need to get motor limits from config here

    // gets called when poc circle is moved. has svg relative
    // co-ordinates
    onPositionUpdate = (svgRelX, svgRelY) => {
        // calculate new leftMotorVel and rightMotorVel
        // REST put
        // Update svg velocity readout
    }
    
    // need to solve the problem of mapping the circle click event x,y pos to svg co-ords, and back.
    
render() {

    return (<div >
        <svg ref={this.svgCanvas} viewBox="-1 -1 2 2" className="MotorControl-svg">
            <line x1="0" y1="-1" x2="0" y2="1" stroke="black" strokeWidth="1" vectorEffect="non-scaling-stroke"/>
            <line x1="-1" y1="0" x2="1" y2="0" stroke="black" strokeWidth="1" vectorEffect="non-scaling-stroke"/>
            <DraggableCircle svgElWidth={0.2} svgElHeight={0.2}/>
        </svg>
    </div>
    );               
  }
}

export default MotorControl;