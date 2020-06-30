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

function respawnPlayer( playerId, randomInt )
	if randomInt == 0 then
		API_send_message(playerId, id, "영은 생을 얻을지이니..." )
		API_SetRespawn_Player(playerId)
	elseif randomInt == 1 then
		API_send_message(playerId, id, "생즉시공, 공즉시색")
	elseif randomInt == 2 then
		API_send_message(playerId, id, "귀신이 돌아다니네..")
	else
		API_send_message(playerId, id, "그런 마음가짐으로 생명을 얻겠느냐?")
	end
end
