#!/usr/bin/ruby

content = ""
while line = gets
        content += line
end

content = content.gsub(/^execve\((0.*, 0.*, 0.*)\)/, 'execve(EXECVE_0, EXECVE_1, EXECVE_2)')

i = 0
content.scan(/0x[0-9a-f]+/).uniq.each do |addr|
        next if addr.match(/0xf{8,}/)       # Probably a negative number

        content = content.gsub(/#{addr}\b/, "ADDR#{i}")
        i = i + 1
end

content = content
        .gsub(/^(brk\b.*=\s+)(-?\d+)/, '\1BRK_RET')
        .gsub(/^arch_prctl\((.*), (.*)\)/, 'arch_prctl(\1, ARCH_PRCTL_2)')
        .gsub(/^(getpp?id\b.*=\s+)(\d+)/, '\1PID_RET')
        .gsub(/^faccessat\((.*, .*, .*), (-?\d+)\)/, 'faccessat(\1, FACCESSAT_4)')

puts content
