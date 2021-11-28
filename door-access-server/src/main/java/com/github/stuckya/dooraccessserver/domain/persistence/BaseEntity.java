package com.github.stuckya.dooraccessserver.domain.persistence;

import com.vladmihalcea.hibernate.type.basic.PostgreSQLEnumType;
import lombok.AccessLevel;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.experimental.SuperBuilder;
import org.hibernate.annotations.TypeDef;
import org.hibernate.annotations.TypeDefs;

import javax.persistence.*;
import java.time.Instant;
import java.util.UUID;

@SuperBuilder
@Getter
@Setter
@NoArgsConstructor
@MappedSuperclass
@TypeDefs({
    @TypeDef(
        name = "pgsql_enum",
        typeClass = PostgreSQLEnumType.class
    )
})
@EntityListeners(BaseEntityPersistenceListener.class)
public abstract class BaseEntity<T extends BaseEntity<T>> {

    @Id
    @Column(updatable = false)
    @GeneratedValue(strategy = GenerationType.AUTO)
    private UUID id;

    @Column(updatable = false)
    private UUID createdBy;

    @Column(updatable = false)
    @Setter(value = AccessLevel.PROTECTED)
    private Instant createdOn;

    private UUID lastModifiedBy;

    @Setter(value = AccessLevel.PROTECTED)
    private Instant lastModifiedOn;

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        T that = (T) o;

        return id != null && id.equals(that.getId());
    }

    @Override
    public int hashCode() {
        return getClass().hashCode();
    }

    public static abstract class BaseEntityBuilder<T extends BaseEntity<T>, C extends BaseEntity<T>, B extends BaseEntityBuilder<T, C, B>> {

        private B createdOn(Instant createdOn) {
            throw new UnsupportedOperationException("createdOn is not supported");
        }

        private B lastModifiedOn(Instant lastModifiedOn) {
            throw new UnsupportedOperationException("lastModifiedOn is not supported");
        }
    }
}
