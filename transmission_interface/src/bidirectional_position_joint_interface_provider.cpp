///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013, PAL Robotics S.L.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of PAL Robotics S.L. nor the names of its
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////

// Pluginlib
#include <pluginlib/class_list_macros.h>

// ros_control
#include <transmission_interface/bidirectional_position_joint_interface_provider.h>

namespace transmission_interface
{

    bool BiDirectionalPositionJointInterfaceProvider::registerTransmission(TransmissionLoaderData& loader_data,
            TransmissionHandleData& handle_data)
    {
        // Setup joint state interface first (if not yet done)
        if (!hasResource(handle_data.name, loader_data.transmission_interfaces.act_to_jnt_state))
        {
            if (!JointStateInterfaceProvider::registerTransmission(loader_data, handle_data)) {return false;}
        }

        if(!hasResource(handle_data.name, loader_data.inverse_transmission_interfaces.jnt_to_act_state))
        {
            if(!loader_data.robot_transmissions->get<transmission_interface::JointToActuatorStateInterface>())
            {
                loader_data.robot_transmissions->registerInterface(&loader_data.inverse_transmission_interfaces.jnt_to_act_state);
            }

            transmission_interface::JointToActuatorStateInterface &interface = *(loader_data.robot_transmissions->get<transmission_interface::JointToActuatorStateInterface>());

            // Update transmission interface
            transmission_interface::JointToActuatorStateHandle handle(handle_data.name, handle_data.transmission.get(), handle_data.act_state_data, handle_data.jnt_state_data);
            interface.registerHandle(handle);
        }

        // If command interface does not yet exist in the robot transmissions, add it and use internal data structures
        if (!loader_data.robot_transmissions->get<transmission_interface::JointToActuatorPositionInterface>())
        {
            loader_data.robot_transmissions->registerInterface(&loader_data.transmission_interfaces.jnt_to_act_pos_cmd);
        }

        if(!loader_data.robot_transmissions->get<transmission_interface::ActuatorToJointPositionInterface>())
        {
            loader_data.robot_transmissions->registerInterface(&loader_data.inverse_transmission_interfaces.act_to_jnt_pos_cmd);
        }

        transmission_interface::JointToActuatorPositionInterface& interface = *(loader_data.robot_transmissions->get<transmission_interface::JointToActuatorPositionInterface>());

        transmission_interface::ActuatorToJointPositionInterface& reverse_interface = *(loader_data.robot_transmissions->get<transmission_interface::ActuatorToJointPositionInterface>());

        // Setup command interface
        transmission_interface::JointToActuatorPositionHandle handle(handle_data.name,
                handle_data.transmission.get(),
                handle_data.act_cmd_data,
                handle_data.jnt_cmd_data);

        transmission_interface::ActuatorToJointPositionHandle reverse_handle(handle_data.name,
                handle_data.transmission.get(),
                handle_data.act_cmd_data,
                handle_data.jnt_cmd_data);

        interface.registerHandle(handle);

        reverse_interface.registerHandle(reverse_handle);

        return true;
    }

}

PLUGINLIB_EXPORT_CLASS(transmission_interface::BiDirectionalPositionJointInterfaceProvider, transmission_interface::RequisiteProvider);
