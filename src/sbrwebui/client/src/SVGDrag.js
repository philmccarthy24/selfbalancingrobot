import React from 'react';

class SVGDrag extends React.Component {

    constructor(props) {
        super(props);
        this.handlePointerDown = this.handlePointerDown.bind(this);
        this.handlePointerMove = this.handlePointerMove.bind(this);
        this.handlePointerUp = this.handlePointerUp.bind(this);
        this.state = {
            x: 0,
            y: 0
        }
    }

    handlePointerDown(event) {
        var rect = event.target.getBoundingClientRect();
        var x = event.clientX - rect.left; // x position within the element.
        var y = event.clientY - rect.top;  // y position within the element.
        console.log("Left? : " + x + " ; Top? : " + y + ".");
    }

    handlePointerMove(event) {

    }

    handlePointerUp(event) {

    }
}