package main

import (
	"fmt"
	"github.com/stuckya/door-access-bus-relay/internal/can"
)

func main() {

	fmt.Println("Starting app...")

	can.SubscribeToBus()
}