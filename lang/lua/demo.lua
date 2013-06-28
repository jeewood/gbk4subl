f = io.open("sublime_text.exe.bak","rb")
if (f) then
	data = f:read("*a")
	f:close()
end

p = "\x85\xC0\x74\x08\x33\xDB\x43"
print ("data.Len:"..string.len(data))

print ("substr.Len:"..string.len(p))
j = 0
for i=1,string.len(data) do
	if (string.byte(data,i)==string.byte(p,1)) then
		for j=1,string.len(p)-1 do
			if (string.byte(data,j+i)~=string.byte(p,j)) then 
				break;
			end
		end
		print(j)
		if j and j>2 then
			print("One ok!"..i)
		end
	end
end

print("failed")
--[[
f = io.open("l.exe","wb")
if (f) then
	f:write(data)
end
]]
