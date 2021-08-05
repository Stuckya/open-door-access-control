package frame

import "github.com/brutella/can"

func NewHeartbeatFrame(id uint8, state [8]uint8) can.Frame {
	return can.Frame{
		Data:  state,
	}
}