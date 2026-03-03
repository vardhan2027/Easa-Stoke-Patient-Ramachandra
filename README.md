This project presents a remote-operated assistive rehabilitation device for stroke patients, designed to support controlled upper-limb movement therapy. The system consists of two primary subsystems: a wireless handheld transmitter (Node) and a receiver-based actuator module.

The Node unit incorporates four tactile push buttons that transmit discrete control commands wirelessly to the receiver. The Receiver unit integrates two servo motor actuators: one dedicated to elbow joint articulation (flexion/extension control) and another mechanism controlling five-finger grasp actuation (open/close function).

Upon button activation, predefined command signals are transmitted from the Node to the Receiver, which interprets and executes precise angular adjustments of the respective servomotors. This enables controlled rehabilitation movement, assisting patients in performing repetitive therapeutic exercises safely and effectively.

The system is designed with the following objectives:

    *Enable remote-controlled motorized arm assistance

    *Provide controlled elbow positioning

    *Facilitate automated finger grasp and release actions

    *Support post-stroke motor rehabilitation therapy

    *Maintain modular wireless communication architecture

The architecture ensures low-latency command transmission, real-time actuator response, and position continuity across resets (non-volatile angle storage implemented on the receiver side).
