class NIFBasic < NIFType
  BOOL_VALUES = {
    "false" => 0,
    "true"  => 1,
    "0" => 0,
    "1" => 1
  }

  def encode_value_of_type(value, desc)
    case name
    when "byte", "char"
      [ Integer(value, 0) ].pack("C")

    when "short"
      [ Integer(value, 0) ].pack("s<")

    when "int"
      [ Integer(value, 0) ].pack("l<")

    when "ushort", "Flags"
      [ Integer(value, 0) ].pack("v")

    when "FileVersion", "ulittle32", "uint"
      [ Integer(value, 0) ].pack("V")

    when "bool"
      [ BOOL_VALUES.fetch(value) ].pack("V")

    when "float"
      [ Float(value) ].pack("e")

    else
      raise "unsupported type in NIFBasic::encode_value_of_type: #{name}"
    end
  end
end
