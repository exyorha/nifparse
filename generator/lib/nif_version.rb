class NIFVersion
  attr_reader :version, :description

  def initialize(version, description)
    @version = version
    @description = description
  end
end
