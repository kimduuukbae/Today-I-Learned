id = -1
function setId( oId )
	id = oId
end

function eventPlayerMove( playerId )
	if API_get_x(playerId) == API_get_x_NPC(id) then
		if API_get_y(playerId) == API_get_y_NPC(id) then
				API_send_message(playerId, id, "HELLO")
		end
	end
end
