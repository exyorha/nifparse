class NIFField
  attr_accessor :name, :type, :ver1, :ver2, :arr1, :arr2, :default, :vercond,
    :cond, :arg, :userver, :userver2, :suffix, :template_type, :description,
    :calculated, :binary, :abstract

  def encode_default(desc)
    type_desc = desc.types.fetch type
    type_desc.encode_value_of_type default, desc
  end
end
